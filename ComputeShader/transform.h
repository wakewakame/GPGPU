#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>

struct uvec3 {
	unsigned int x;
	unsigned int y;
	unsigned int z;
	uvec3()
	{
		x = 1;
		y = 1;
		z = 1;
	}
};

class Code
{
private:
	class Chart {
	public:
		enum class NodeType : char
		{
			ROOT, // コード全体
			LINE, // 普通の行
			SS_COMMENT, // //コメント
			SA_COMMENT, // /**/コメント
			QUOTATION, // ''
			DQUOTATION, // ""
			PARENTHESRS, // ()
			BRACKETS, // []
			BRACES, // {}
			SSBO, // SSBO
			MAIN, // MAIN
			INDEX // INDEX
		};
		struct DEF
		{
			std::string name;
			std::string val;
		};

		NodeType type;
		bool colon; // 文末にコロンが必要かどうか
		bool semicolon; // 文末にセミコロンが必要かどうか
		std::string start;
		std::string end;
		std::string version;

		std::string text;
		Chart *parent;
		std::vector<Chart*> childs;
		int index;
		uvec3 loop;
		uvec3 sled;
		std::vector<unsigned int> bind;
		std::vector<DEF> def;

		Chart *Add(NodeType setType)
		{
			childs.push_back(new Chart(setType));
			childs[childs.size() - 1]->parent = this;
			childs[childs.size() - 1]->index = childs.size() - 1;
			return childs[childs.size() - 1];
		}

		Chart(NodeType setType)
		{
			type = setType;
			semicolon = 0;
			start = "";
			end = "";
			version = "430";
			colon = 0;
			text = "";
			parent = nullptr;
			childs.clear();
			index = 0;
			bind.clear();
			def.clear();
		}

		~Chart()
		{
			for (int i = 0; i < childs.size(); i++)
			{
				delete childs[i];
			}
		}
	};

	class Read : public Chart
	{
	private:
		std::vector<std::string> code;
		unsigned int bind_index;

		bool charExist(std::string &buf, int index, char f)
		{
			if (index < 0 || index >= buf.size()) return 0;
			if (buf[index] != f) return 0;
			return 1;
		}

		void Arrange(Chart *Scope)
		{
			bool first_empty = 1;
			std::string str = "";
			switch (Scope->type)
			{
			case Chart::NodeType::LINE:

				for (int i = 0; i < Scope->text.size(); i++)
				{
					switch (Scope->text[i])
					{
					case '\t':
						break;
					case ' ':
						if (first_empty) break;
						else if (charExist(str, str.size() - 1, ' ')) break;
						else str += Scope->text[i];
						break;
					default:
						first_empty = 0;
						str += Scope->text[i];
						break;
					}
				}
				Scope->text = str;
				break;
			case Chart::NodeType::ROOT:
			case Chart::NodeType::PARENTHESRS:
			case Chart::NodeType::BRACES:
			case Chart::NodeType::BRACKETS:
				for (int i = 0; i < Scope->childs.size(); i++)
				{
					Arrange(Scope->childs[i]);
				}
				break;
			case Chart::NodeType::SS_COMMENT:
			case Chart::NodeType::SA_COMMENT:
			case Chart::NodeType::QUOTATION:
			case Chart::NodeType::DQUOTATION:
				break;
			}
		}

		void Transform(Chart *Scope)
		{
			unsigned int char_index = -1;
			bool index_flag = 1;
			std::string str = "";
			switch (Scope->type)
			{
			case Chart::NodeType::LINE:
				if (Scope->text.substr(0, 4) == "main")
				{
					Scope->type = Chart::NodeType::MAIN;
					Scope->text = "layout(local_size_x = " + std::to_string((int)ceil(((float)loop.x) / ((float)sled.x))) + ", " + "local_size_y = " + std::to_string((int)ceil(((float)loop.y) / ((float)sled.y))) + ", " + "local_size_z = " + std::to_string((int)ceil(((float)loop.z) / ((float)sled.z))) + ") in;\nvoid main";
					if (Scope->index + 2 < Scope->parent->childs.size()) {
						if (Scope->parent->childs[Scope->index + 2]->type == Chart::NodeType::BRACES) {
							Scope->parent->childs[Scope->index + 2]->start += "\nif(gl_GlobalInvocationID.x >= " + std::to_string(this->loop.x) + " || gl_GlobalInvocationID.y >= " + std::to_string(this->loop.y) + " || gl_GlobalInvocationID.z >= " + std::to_string(this->loop.z) + ") return;\n";
						}
					}
				}
				if (Scope->text.substr(0, 4) == "SSBO")
				{
					if (Scope->index != Scope->parent->childs.size() - 1)
					{
						if (Scope->parent->childs[Scope->index + 1]->type == Chart::NodeType::BRACES)
						{
							for (int i = std::string("SSBO").size(); i < Scope->text.size(); i++)
							{
								switch (Scope->text[i])
								{
								case ' ':
								case 10:
								case 13:
									break;
								default:
									Scope->parent->childs[Scope->index + 1]->end += Scope->text[i];
									break;
								}
							}
						}
					}

					Scope->type = Chart::NodeType::SSBO;
					if (bind_index >= bind.size())
					{
						Scope->text = "layout(std430,binding=0) buffer SSBO0";
					}
					else
					{
						Scope->text = "layout(std430,binding=" + std::to_string(bind[bind_index]) + ") buffer SSBO" + std::to_string(bind[bind_index]);
					}
					bind_index += 1;
				}
				char_index = -1;
				while (true)
				{
					str = "";
					index_flag = 1;
					char_index = Scope->text.find("index", char_index + 1);
					if (char_index != -1)
					{
						if (char_index != 0)
						{
							switch (Scope->text[char_index - 1])
							{
							case ' ':
							case '+':
							case '-':
							case '*':
							case '/':
							case '%':
							case ';':
							case '=':
							case '<':
							case '>':
							case '!':
							case '.':
							case 10:
							case 13:
								str += Scope->text.substr(0, char_index);
								break;
							default:
								index_flag = 0;
								break;
							}
						}
						str += "gl_GlobalInvocationID";
						if (char_index + std::string("index").size() - 1 != Scope->text.size() - 1)
						{
							switch (Scope->text[char_index + std::string("index").size()])
							{
							case ' ':
							case '+':
							case '-':
							case '*':
							case '/':
							case '%':
							case ';':
							case '=':
							case '<':
							case '>':
							case '!':
							case '.':
							case 10:
							case 13:
								str += Scope->text.substr(char_index + std::string("index").size(), Scope->text.size() - (char_index + std::string("index").size()));
								break;
							default:
								index_flag = 0;
								break;
							}
						}
						if (index_flag)
						{
							Scope->type = Chart::NodeType::INDEX;
							Scope->text = str;
						}
					}
					else {
						index_flag = 0;
						break;
					}
				}
				char_index = -1;
				while (true)
				{
					str = "";
					index_flag = 1;
					char_index = Scope->text.find("loop", char_index + 1);
					if (char_index != -1)
					{
						if (char_index != 0)
						{
							switch (Scope->text[char_index - 1])
							{
							case ' ':
							case '+':
							case '-':
							case '*':
							case '/':
							case '%':
							case ';':
							case '=':
							case '<':
							case '>':
							case '!':
							case '.':
							case 10:
							case 13:
								str += Scope->text.substr(0, char_index);
								break;
							default:
								index_flag = 0;
								break;
							}
						}
						str += "ivec3(" + std::to_string(loop.x) + ", " + std::to_string(loop.y) + ", " + std::to_string(loop.z) + ")";
						if (char_index + std::string("loop").size() - 1 != Scope->text.size() - 1)
						{
							switch (Scope->text[char_index + std::string("loop").size()])
							{
							case ' ':
							case '+':
							case '-':
							case '*':
							case '/':
							case '%':
							case ';':
							case '=':
							case '<':
							case '>':
							case '!':
							case '.':
							case 10:
							case 13:
								str += Scope->text.substr(char_index + std::string("loop").size(), Scope->text.size() - (char_index + std::string("loop").size()));
								break;
							default:
								index_flag = 0;
								break;
							}
						}
						if (index_flag)
						{
							Scope->type = Chart::NodeType::INDEX;
							Scope->text = str;
						}
					}
					else {
						index_flag = 0;
						break;
					}
				}
			case Chart::NodeType::ROOT:
			case Chart::NodeType::PARENTHESRS:
			case Chart::NodeType::BRACKETS:
			case Chart::NodeType::BRACES:
				for (int i = 0; i < Scope->childs.size(); i++)
				{
					Transform(Scope->childs[i]);
				}
				break;
			}
		}

		std::string Write(Chart *chart, bool return_flag = 1)
		{
			std::string res = "";

			switch (chart->type)
			{
			case Chart::NodeType::ROOT:
				for (int i = 0; i < chart->childs.size(); i++) {
					res = res + Write(chart->childs[i], 1);
				}
				break;
			case Chart::NodeType::BRACES:
				res = res + "{" + chart->start;
				for (int i = 0; i < chart->childs.size(); i++) {
					if (i == 0) res = res + "\n";
					res = res + Write(chart->childs[i]);
				}
				res = res + "}";
				break;
			case Chart::NodeType::PARENTHESRS:
				res = res + "(";
				for (int i = 0; i < chart->childs.size(); i++) {
					res = res + Write(chart->childs[i], 0);
				}
				res = res + ")";
				break;
			case Chart::NodeType::BRACKETS:
				res = res + "[";
				for (int i = 0; i < chart->childs.size(); i++) {
					res = res + Write(chart->childs[i], 0);
				}
				res = res + "]";
				break;
			case Chart::NodeType::QUOTATION:
				res = res + "\n'" + chart->text + "'";
				break;
			case Chart::NodeType::DQUOTATION:
				res = res + "\n\"" + chart->text + "\"";
				break;
			case Chart::NodeType::LINE:
				res = res + chart->text;
				break;
			case Chart::NodeType::MAIN:
				res = res + chart->text;
				break;
			case Chart::NodeType::SSBO:
				res = res + chart->text;
				break;
			case Chart::NodeType::INDEX:
				res = res + chart->text;
				break;
			case Chart::NodeType::SS_COMMENT:
				res = res + "//" + chart->text;
				break;
			case Chart::NodeType::SA_COMMENT:
				res = res + "\n/*" + chart->text + "*/";
				break;
			}
			res = res + chart->end;
			if (chart->colon) res = res + ",";
			if (chart->semicolon) res = res + ";";
			if (return_flag)
			{
				if (chart->parent != nullptr)
				{
					if (chart->index != chart->parent->childs.size() - 1) {
						if (
							chart->parent->childs[index + 1]->type != Chart::NodeType::PARENTHESRS &&
							chart->parent->childs[index + 1]->type != Chart::NodeType::BRACKETS &&
							chart->parent->childs[index + 1]->type != Chart::NodeType::QUOTATION &&
							chart->parent->childs[index + 1]->type != Chart::NodeType::DQUOTATION
							)
						{
							res = res + "\n";
						}
					}
					else {
						res = res + "\n";
					}
				}
			}
			return res;
		}

	public:
		void Load()
		{
			bind_index = 0;
			Chart *Scope = this;
			bool skip = 0, back = 0;
			for (int line = 0; line < code.size(); line++)
			{
				for (int index = 0; index < code[line].size(); index++)
				{
					if (skip)
					{
						skip = 0;
						continue;
					}
					if (back)
					{
						index -= 1;
						back = 0;
					}
					switch (Scope->type)
					{
						// ROOT
					case Chart::NodeType::ROOT:
						switch (code[line][index]) {
						case ' ':
						case '\t':
						case 10:
						case 13:
							break;
						case '/':
							if (charExist(code[line], index + 1, '/'))
							{
								Scope = Scope->Add(Chart::NodeType::SS_COMMENT);
								skip = 1;
								break;
							}
							else if (charExist(code[line], index + 1, '*'))
							{
								Scope = Scope->Add(Chart::NodeType::SA_COMMENT);
								skip = 1;
								break;
							}
							else
							{
								Scope = Scope->Add(Chart::NodeType::LINE);
								back = 1;
								break;
							}
						case '\'':
							Scope = Scope->Add(Chart::NodeType::QUOTATION);
							break;
						case '"':
							Scope = Scope->Add(Chart::NodeType::DQUOTATION);
							break;
						case '(':
							Scope = Scope->Add(Chart::NodeType::PARENTHESRS);
							break;
						case '[':
							Scope = Scope->Add(Chart::NodeType::BRACKETS);
							break;
						case '{':
							Scope = Scope->Add(Chart::NodeType::BRACES);
							break;
						case ',':
							Scope->childs[Scope->childs.size() - 1]->colon = 1;
							break;
						case ';':
							Scope->childs[Scope->childs.size() - 1]->semicolon = 1;
							break;
						default:
							back = 1;
							Scope = Scope->Add(Chart::NodeType::LINE);
							break;
						}
						break;
						// LINE
					case Chart::NodeType::LINE:
						switch (code[line][index]) {
						case '/':
							if (charExist(code[line], index + 1, '/'))
							{
								Scope = Scope->parent->Add(Chart::NodeType::SS_COMMENT);
								skip = 1;
								break;
							}
							else if (charExist(code[line], index + 1, '*'))
							{
								Scope = Scope->parent->Add(Chart::NodeType::SA_COMMENT);
								skip = 1;
								break;
							}
							else
							{
								Scope->text += code[line][index];
								if (index + 1 == code[line].size())
								{
									Scope = Scope->parent->Add(Chart::NodeType::LINE);
								}
								break;
							}
						case '\'':
							Scope = Scope->parent->Add(Chart::NodeType::QUOTATION);
							break;
						case '"':
							Scope = Scope->parent->Add(Chart::NodeType::DQUOTATION);
							break;
						case '(':
							Scope = Scope->parent->Add(Chart::NodeType::PARENTHESRS);
							break;
						case ')':
							if (Scope->parent->type == Chart::NodeType::PARENTHESRS)
							{
								Scope = Scope->parent->parent;
							}
							else {
								Scope->text += code[line][index];
								if (index + 1 == code[line].size())
								{
									Scope = Scope->parent->Add(Chart::NodeType::LINE);
								}
							}
							break;
						case '[':
							Scope = Scope->parent->Add(Chart::NodeType::BRACKETS);
							break;
						case ']':
							if (Scope->parent->type == Chart::NodeType::BRACKETS)
							{
								Scope = Scope->parent->parent;
							}
							else {
								Scope->text += code[line][index];
								if (index + 1 == code[line].size())
								{
									Scope = Scope->parent->Add(Chart::NodeType::LINE);
								}
							}
							break;
						case '{':
							Scope = Scope->parent->Add(Chart::NodeType::BRACES);
							break;
						case '}':
							if (Scope->parent->type == Chart::NodeType::BRACES)
							{
								Scope = Scope->parent->parent;
							}
							else {
								Scope->text += code[line][index];
								if (index + 1 == code[line].size())
								{
									Scope = Scope->parent->Add(Chart::NodeType::LINE);
								}
							}
							break;
						case ',':
							Scope->colon = 1;
							Scope = Scope->parent->Add(Chart::NodeType::LINE);
							break;
						case ';':
							Scope->semicolon = 1;
							Scope = Scope->parent->Add(Chart::NodeType::LINE);
							break;
						case 10:
						case 13:
							Scope = Scope->parent;
							break;
						default:
							Scope->text += code[line][index];
							if (index + 1 == code[line].size())
							{
								Scope = Scope->parent;
							}
							break;
						}
						break;
						// SS_COMMENT
					case Chart::NodeType::SS_COMMENT:
						switch (code[line][index]) {
						case 10:
						case 13:
							Scope = Scope->parent->Add(Chart::NodeType::LINE);
							break;
						default:
							Scope->text += code[line][index];
							if (index + 1 == code[line].size())
							{
								Scope = Scope->parent->Add(Chart::NodeType::LINE);
							}
							break;
						}
						// SA_COMMENT
					case Chart::NodeType::SA_COMMENT:
						switch (code[line][index]) {
						case 10:
						case 13:
							Scope->text += "\n";
							break;
						case '*':
							if (charExist(code[line], index + 1, '/'))
							{
								Scope = Scope->parent;
								skip = 1;
								break;
							}
							else
							{
								Scope->text += code[line][index];
								if (index + 1 == code[line].size())
								{
									Scope->text += "\n";
								}
								break;
							}
						default:
							Scope->text += code[line][index];
							if (index + 1 == code[line].size())
							{
								Scope->text += "\n";
							}
							break;
						}
						break;
						// QUOTATION
					case Chart::NodeType::QUOTATION:
						switch (code[line][index]) {
						case 10:
						case 13:
							Scope->text += "\n";
							break;
						case '\'':
							if (!charExist(code[line], index - 1, '\\'))
							{
								Scope = Scope->parent;
								break;
							}
							else
							{
								Scope->text += code[line][index];
								break;
							}
						default:
							Scope->text += code[line][index];
							break;
						}
						break;
						// DQUOTATION
					case Chart::NodeType::DQUOTATION:
						switch (code[line][index]) {
						case 10:
						case 13:
							Scope->text += "\n";
							break;
						case '"':
							if (!charExist(code[line], index - 1, '\\'))
							{
								Scope = Scope->parent;
								break;
							}
							else
							{
								Scope->text += code[line][index];
								break;
							}
						default:
							Scope->text += code[line][index];
							break;
						}
						break;
						// PARENTHESRS
					case Chart::NodeType::PARENTHESRS:
						switch (code[line][index]) {
						case ' ':
						case '\t':
						case 10:
						case 13:
							break;
						case '/':
							if (charExist(code[line], index + 1, '/'))
							{
								Scope = Scope->Add(Chart::NodeType::SS_COMMENT);
								skip = 1;
								break;
							}
							else if (charExist(code[line], index + 1, '*'))
							{
								Scope = Scope->Add(Chart::NodeType::SA_COMMENT);
								skip = 1;
								break;
							}
							else
							{
								Scope = Scope->Add(Chart::NodeType::LINE);
								back = 1;
								break;
							}
						case '\'':
							Scope = Scope->Add(Chart::NodeType::QUOTATION);
							break;
						case '"':
							Scope = Scope->Add(Chart::NodeType::DQUOTATION);
							break;
						case '(':
							Scope = Scope->Add(Chart::NodeType::PARENTHESRS);
							break;
						case ')':
							Scope = Scope->parent;
							break;
						case '[':
							Scope = Scope->Add(Chart::NodeType::BRACKETS);
							break;
						case '{':
							Scope = Scope->Add(Chart::NodeType::BRACES);
							break;
						case ',':
							Scope->childs[Scope->childs.size() - 1]->colon = 1;
							break;
						case ';':
							Scope->childs[Scope->childs.size() - 1]->semicolon = 1;
							break;
						default:
							Scope = Scope->Add(Chart::NodeType::LINE);
							back = 1;
							break;
						}
						break;
						// BRACKETS
					case Chart::NodeType::BRACKETS:
						switch (code[line][index]) {
						case ' ':
						case '\t':
						case 10:
						case 13:
							break;
						case '/':
							if (charExist(code[line], index + 1, '/'))
							{
								Scope = Scope->Add(Chart::NodeType::SS_COMMENT);
								skip = 1;
								break;
							}
							else if (charExist(code[line], index + 1, '*'))
							{
								Scope = Scope->Add(Chart::NodeType::SA_COMMENT);
								skip = 1;
								break;
							}
							else
							{
								Scope = Scope->Add(Chart::NodeType::LINE);
								back = 1;
								break;
							}
						case '\'':
							Scope = Scope->Add(Chart::NodeType::QUOTATION);
							break;
						case '"':
							Scope = Scope->Add(Chart::NodeType::DQUOTATION);
							break;
						case '(':
							Scope = Scope->Add(Chart::NodeType::PARENTHESRS);
							break;
						case '[':
							Scope = Scope->Add(Chart::NodeType::BRACKETS);
							break;
						case ']':
							Scope = Scope->parent;
							break;
						case '{':
							Scope = Scope->Add(Chart::NodeType::BRACES);
							break;
						case ',':
							Scope->childs[Scope->childs.size() - 1]->colon = 1;
							break;
						case ';':
							Scope->childs[Scope->childs.size() - 1]->semicolon = 1;
							break;
						default:
							Scope = Scope->Add(Chart::NodeType::LINE);
							back = 1;
							break;
						}
						break;
						// BRACES
					case Chart::NodeType::BRACES:
						switch (code[line][index]) {
						case ' ':
						case '\t':
						case 10:
						case 13:
							break;
						case '/':
							if (charExist(code[line], index + 1, '/'))
							{
								Scope = Scope->Add(Chart::NodeType::SS_COMMENT);
								skip = 1;
								break;
							}
							else if (charExist(code[line], index + 1, '*'))
							{
								Scope = Scope->Add(Chart::NodeType::SA_COMMENT);
								skip = 1;
								break;
							}
							else
							{
								Scope = Scope->Add(Chart::NodeType::LINE);
								back = 1;
								break;
							}
						case '\'':
							Scope = Scope->Add(Chart::NodeType::QUOTATION);
							break;
						case '"':
							Scope = Scope->Add(Chart::NodeType::DQUOTATION);
							break;
						case '(':
							Scope = Scope->Add(Chart::NodeType::PARENTHESRS);
							break;
						case '[':
							Scope = Scope->Add(Chart::NodeType::BRACKETS);
							break;
						case '{':
							Scope = Scope->Add(Chart::NodeType::BRACES);
							break;
						case '}':
							Scope = Scope->parent;
							break;
						case ',':
							Scope->childs[Scope->childs.size() - 1]->colon = 1;
							break;
						case ';':
							Scope->childs[Scope->childs.size() - 1]->semicolon = 1;
							break;
						default:
							Scope = Scope->Add(Chart::NodeType::LINE);
							back = 1;
							break;
						}
						break;
					}
				}
			}
			Arrange(this);
			Transform(this);
		}

		bool Open(std::string text, bool FileMode)
		{
			if (FileMode)
			{
				code.clear();
				std::string str;
				std::ifstream ifsText(text);
				if (ifsText.fail())
				{
					return 0;
				}

				while (std::getline(ifsText, str))
				{
					code.push_back(str);
				}
			}
			else
			{
				code.clear();
				std::string str;

				for (int i = 0; i < text.size(); i++) {
					switch (text[i])
					{
					case 10:
					case 13:
						code.push_back(str);
						str = "";
						break;
					default:
						str += text[i];
						break;
					}
				}
				code.push_back(str);
			}
			return 1;
		}

		Read() : Chart(Chart::NodeType::ROOT)
		{
		}

		std::string getGLSL()
		{
			std::string res = "";
			res += "#version " + version + "\n";
			for (int i = 0; i < def.size(); i++)
			{
				res += "#define " + def[i].name + " " + def[i].val + "\n";
			}
			Load();
			res += Write(this);
			return res;
		}

		void bind_point(std::vector<unsigned int> set_bind)
		{
			bind = set_bind;
		}
	};

	Read read;

public:
	bool open(std::string text, bool FileMode)
	{
		return read.Open(text, FileMode);
	}

	void set_param(std::string version, uvec3 loop, uvec3 sled, std::vector<unsigned int> set_bind)
	{
		read.version = version;
		read.loop = loop;
		read.sled = sled;
		read.bind_point(set_bind);
	}

	void set_define(std::string def_name, std::string def_val)
	{
		for (int i = 0; i < read.def.size(); i++)
		{
			if (def_name == read.def[i].name)
			{
				read.def[i].val = def_val;
				return;
			}
		}
		read.def.push_back({ def_name, def_val });
		return;
	}

	std::string get_glsl()
	{
		return read.getGLSL();
	}
};