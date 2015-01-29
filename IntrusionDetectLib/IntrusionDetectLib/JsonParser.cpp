#include "JsonParser.h"

//从文件解析json
int JsonParser::ParseJsonFromFile(const char* filename)  
{  
  // 解析json用Json::Reader  
  Json::Reader reader;  
  // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
  Json::Value root;         

  std::ifstream is;  
  is.open (filename, std::ios::binary );    
  if (reader.parse(is, root))  
  {  
	  parse(root);
  }  
  is.close();  
  return 0;  
} 

int JsonParser::parseJsonFromString(const char* str){
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(str, root)){
		parse(root);
	}
	return 0;
}

int JsonParser::parse(Json::Value root){
	std::string code;
	if (!root["version"].isNull())  // 访问节点，Access an object value by name, create a null member if it does not exist.  
		code = root["version"].asString();

	// 访问节点，Return the member named key if it exist, defaultValue otherwise.  
	code = root.get("version", "null").asString();

	// 得到"alg-params"的数组个数  
	params_size = root["alg-params"].size();

	// 遍历数组  
	for (int i = 0; i < params_size; ++i)
	{
		std::string str_temp;
		Json::Value val_direction = root["alg-params"][i]["direction"];  //direction
		params[i].direction_size = val_direction.size();
		for (int j = 0; j < params[i].direction_size; ++j)
		{
			str_temp = val_direction[j].asString();

			if ("all" == str_temp){
				params[i].direction[j] = 0;
			}
			else if ("up2down" == str_temp){
				params[i].direction[j] = 1;
			}
			else if ("down2up" == str_temp){
				params[i].direction[j] = 2;
			}
			else if ("left2right" == str_temp){
				params[i].direction[j] = 3;
			}
			else if ("right2left" == str_temp){
				params[i].direction[j] = 4;
			}
			else{
				return false;
			}
		}
		if (!root["alg-params"][i]["target"].isNull())
		{
			Json::Value val_target = root["alg-params"][i]["target"];
			// 访问节点，Return the member named key if it exist, defaultValue otherwise.  
			//  code = root.get("uploadid", "null").asString();  

			std::string str_target = val_target[0].asString();   //target
			//str_temp = val_target.;	  
			if ("all" == str_target){
				params[i].target = 0;
			}
			else if ("person" == str_target){
				params[i].target = 1;
			}
			else if ("animal" == str_target){
				params[i].target = 2;
			}
			else{
				return false;
			}
		}

		Json::Value val_area = root["alg-params"][i]["areas"];       //areas
		params[i].areas_size = val_area.size();
		for (int j = 0; j < params[i].areas_size; ++j)
		{
			str_temp = val_area[j]["left"].asString();
			params[i].areas[j].left = atoi(str_temp.c_str());
			str_temp = val_area[j]["top"].asString();
			params[i].areas[j].top = atoi(str_temp.c_str());
			str_temp = val_area[j]["width"].asString();
			params[i].areas[j].width = atoi(str_temp.c_str());
			str_temp = val_area[j]["height"].asString();
			params[i].areas[j].height = atoi(str_temp.c_str());
		}
	}
	return 0;
}