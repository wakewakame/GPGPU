#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>

class Code
{
private:
	class Chart{
	public:
		enum class NodeType : char
		{
			ROOT, // コード全体
			LINE, // 普通の行
			SHARP, // #行
			SS_COMMENT, // //コメント
			SA_COMMENT, // /**/コメント
			QUOTATION, // ''
			DQUOTATION, // ""
			PARENTHESRS, // ()
			BRACKETS, // []
			BRACES // {}
		} type;
		bool colon; // 文末にコロンが必要かどうか
		bool semicolon; // 文末にセミコロンが必要かどうか

		std::string text;
		Chart *parent;
		std::vector<Chart*> childs;

		Chart *Add(NodeType setType)
		{
			childs.push_back(new Chart(setType));
			childs[childs.size() - 1]->parent = this;
			return childs[childs.size() - 1];
		}

		Chart(NodeType setType)
		{
			type = setType;
			semicolon = 0;
			colon = 0;
			text = "";
			parent = nullptr;
			childs.clear();
		}

		~Chart()
		{
			for(int i = 0; i < childs.size(); i++)
			{
				delete childs[i];
			}
		}
	};

	class Read : public Chart
	{
	private:
		bool charExist(std::string &buf, int index, char f)
		{
			if (index < 0 || index >= buf.size()) return 0;
			if (buf[index] != f) return 0;
			return 1;
		}

		Chart *FindUpClose(Chart *self)
		{
			if(self->parent != nullptr)
			{
				if(
					self->parent->type == Chart::NodeType::ROOT ||
					self->parent->type == Chart::NodeType::SA_COMMENT ||
					self->parent->type == Chart::NodeType::QUOTATION ||
					self->parent->type == Chart::NodeType::DQUOTATION ||
					self->parent->type == Chart::NodeType::PARENTHESRS ||
					self->parent->type == Chart::NodeType::BRACKETS ||
					self->parent->type == Chart::NodeType::BRACES
				)
				{
					return self->parent;
				}
				else
				{
					return FindUpClose(self->parent);
				}
			}
			else{
				return nullptr;
			}
		}

	public:
		bool Load(std::string Filename)
		{
			std::string str;
			std::ifstream ifsText(Filename);
			if (ifsText.fail())
			{
				return 0;
			}
			
			Chart *Scope = (Chart*)this;
			bool continue_flag = 0;
			while (std::getline(ifsText, str))
			{
				for (int i = 0; i < (int)str.size(); i++)
				{
					if (continue_flag)
					{
						continue_flag = 0;
						continue;
					}
					switch(Scope->type)
					{
					case Chart::NodeType::ROOT:
					case Chart::NodeType::BRACES:
						switch(str[i])
						{
							case ' ':
							case '\t':
								break;
							case '#':
								Scope = Scope->Add(Chart::NodeType::SHARP);
								break;
							case '/':
								if (charExist(str, i+1, '/'))
								{
									Scope = Scope->Add(Chart::NodeType::SS_COMMENT);
									continue_flag = 1;
								}
								else if (charExist(str, i+1, '*'))
								{
									Scope = Scope->Add(Chart::NodeType::SA_COMMENT);
									continue_flag = 1;
								}
								else 
								{
									Scope = Scope->Add(Chart::NodeType::LINE);
									Scope->text += str[i];
								}
								break;
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
								if (Scope->type == Chart::NodeType::BRACES)
								{
									Scope = Scope->parent;
								}
								break;
							case ';':
								Scope->childs[Scope->childs.size()-1]->semicolon = 1;
								break;
							case ',':
								Scope->childs[Scope->childs.size()-1]->colon = 1;
								break;
							default:
								Scope = Scope->Add(Chart::NodeType::LINE);
								Scope->text += str[i];
								break;
						}
						break;
					case Chart::NodeType::PARENTHESRS:
						switch(str[i])
						{
							case ' ':
								if (Scope->text.size() == 0) break;
								else
								{
									Scope = Scope->Add(Chart::NodeType::LINE);
									Scope->text += str[i];
								}
								break;
							case '\t':
								break;
							case '/':
								if (charExist(str, i+1, '/'))
								{
									Scope = Scope->Add(Chart::NodeType::SS_COMMENT);
									continue_flag = 1;
								}
								else if (charExist(str, i+1, '*'))
								{
									Scope = Scope->Add(Chart::NodeType::SA_COMMENT);
									continue_flag = 1;
								}
								else
								{
									Scope = Scope->Add(Chart::NodeType::LINE);
									Scope->text += str[i];
								}
								break;
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
							case ';':
								Scope->childs[Scope->childs.size()-1]->semicolon = 1;
								break;
							case ',':
								Scope->childs[Scope->childs.size()-1]->colon = 1;
								break;
							default:
								Scope = Scope->Add(Chart::NodeType::LINE);
								Scope->text += str[i];
								break;
						}
						break;
					case Chart::NodeType::LINE:
						switch(str[i])
						{
							case ' ':
								if (
									Scope->text.size() == 0 ||
									charExist(Scope->text, Scope->text.size()-1, ' ')
								) break;
								else
								{
									Scope->text += str[i];
									if (i + 1 == str.size())
									{
										Scope = FindUpClose(Scope);
									}
								}
								break;
							case '\t':
								break;
							case '/':
								if (charExist(str, i+1, '/'))
								{
									Scope = FindUpClose(Scope)->Add(Chart::NodeType::SS_COMMENT);
									continue_flag = 1;
								}
								else if (charExist(str, i+1, '*'))
								{
									Scope = FindUpClose(Scope)->Add(Chart::NodeType::SA_COMMENT);
									continue_flag = 1;
								}
								else
								{
									Scope->text += str[i];
									if (i + 1 == str.size())
									{
										Scope = FindUpClose(Scope);
									}
								}
								break;
							case '\'':
								Scope = FindUpClose(Scope)->Add(Chart::NodeType::QUOTATION);
								break;
							case '"':
								Scope = FindUpClose(Scope)->Add(Chart::NodeType::DQUOTATION);
								break;
							case '(':
								Scope = FindUpClose(Scope)->Add(Chart::NodeType::PARENTHESRS);
								break;
							case '[':
								Scope = FindUpClose(Scope)->Add(Chart::NodeType::BRACKETS);
								break;
							case '{':
								Scope = FindUpClose(Scope)->Add(Chart::NodeType::BRACES);
								break;
							case ';':
								Scope->semicolon = 1;
								Scope = FindUpClose(Scope);
								break;
							case ',':
								if(Scope->parent->type == Chart::NodeType::PARENTHESRS)
								{
									Scope->colon = 1;
									Scope = FindUpClose(Scope);
									break;
								}
							default:
								Scope->text += str[i];
								if (i + 1 == str.size())
								{
									Scope = FindUpClose(Scope);
								}
								break;
						}
						break;
					case Chart::NodeType::SHARP:
						Scope->text += str[i];
						if (i + 1 == str.size())
						{
							Scope = FindUpClose(Scope);
						}
						break;
					case Chart::NodeType::SS_COMMENT:
						Scope->text += str[i];
						if (i + 1 == str.size())
						{
							Scope = FindUpClose(Scope);
						}
						break;
					case Chart::NodeType::SA_COMMENT:
						switch(str[i])
						{
							case '*':
								if (charExist(str, i+1, '/'))
								{
									Scope = Scope->parent;
									continue_flag = 1;
								}
								else Scope->text += str[i];
								break;
							default:
								Scope->text += str[i];
								if (i + 1 == str.size())
								{
									Scope->text += "\n";
								}
								break;
						}
						break;
					case Chart::NodeType::QUOTATION:
						switch(str[i])
						{
							case '\'':
								if (!charExist(Scope->text, Scope->text.size()-1, '\\')) Scope = Scope->parent;
								else
								{
									Scope->text += str[i];
									if (i + 1 == str.size())
									{
										Scope->text += "\n";
									}
								}
								break;
							default:
								Scope->text += str[i];
								break;
						}
						break;
					case Chart::NodeType::DQUOTATION:
						switch(str[i])
						{
							case '"':
								if (!charExist(Scope->text, Scope->text.size()-1, '\\')) Scope = Scope->parent;
								else
								{
									Scope->text += str[i];
									if (i + 1 == str.size())
									{
										Scope->text += "\n";
									}
								}
								break;
							default:
								Scope->text += str[i];
								break;
						}
						break;
					case Chart::NodeType::BRACKETS:
						switch(str[i])
						{
							case ']':
								Scope = Scope->parent;
								break;
							default:
								Scope->text += str[i];
								if (i + 1 == str.size())
								{
									Scope->text += "\n";
								}
								break;
						}
						break;
					}
				}
			}
			return 1;
		}

		Read(std::string Filename) : Chart(Chart::NodeType::ROOT)
		{
			Load(Filename);
		}
	};

	class Write
	{
	private:
		std::string code;

		std::string Load(Chart *chart)
		{
			std::string res = "";

			switch(chart->type)
			{
			case Chart::NodeType::ROOT:
				for (int  i = 0; i < chart->childs.size(); i++){
					res += Load(chart->childs[i]);
				}
				break;
			case Chart::NodeType::BRACES:
				//res += "{";
				for (int  i = 0; i < chart->childs.size(); i++){
					res += Load(chart->childs[i]);
				}
				//res += "}"
				break;
			case Chart::NodeType::PARENTHESRS:
				//res += "(";
				for (int  i = 0; i < chart->childs.size(); i++){
					res += Load(chart->childs[i]);
				}
				//res += "(";
				break;
			case Chart::NodeType::QUOTATION:
				res += "'" + chart->text + "'";
				break;
			case Chart::NodeType::DQUOTATION:
				res += "\"" + chart->text + "\"";
				break;
			case Chart::NodeType::BRACKETS:
				res += "[" + chart->text + "]";
				break;
			case Chart::NodeType::LINE:
				res += chart->text;
				break;
			case Chart::NodeType::SHARP:
				res += "#" + chart->text;
				break;
			case Chart::NodeType::SS_COMMENT:
				res += "//" + chart->text;
				break;
			case Chart::NodeType::SA_COMMENT:
				res += "/*" + chart->text + "*/";
				break;
			}
			if (chart->colon) res += ",";
			if (chart->semicolon) res += ";";

			res += "\n";

			return res;
		}

	public:
		Write(Chart &chart)
		{
			code = "";
			code = Load(chart);
			std::cout << code << std::endl;
		}
	};

public:
	Code(std::string Filename)
	{
		Read a(Filename);
		Write b(a);
	}
};

int main(){
	Code c("p1.gpu");
	return 1;
}