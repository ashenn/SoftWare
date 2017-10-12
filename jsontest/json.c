#include <json/json.h>
#include <stdio.h>

int main() {
  /*Creating a json object*/
  json_object * jobj = json_object_new_object();

  /*Creating a json string*/
  json_object *jstring = json_object_new_string("Joys of Programming");

  /*Creating a json integer*/
  json_object *jint = json_object_new_int(10);

  /*Creating a json boolean*/
  json_object *jboolean = json_object_new_boolean(1);

  /*Creating a json double*/
  json_object *jdouble = json_object_new_double(2.14);

  /*Creating a json array*/
  json_object *jarray = json_object_new_array();

  /*Creating json strings*/
  json_object *jstring1 = json_object_new_string("c");
  json_object *jstring2 = json_object_new_string("c++");
  json_object *jstring3 = json_object_new_string("php");

  /*Adding the above created json strings to the array*/
  json_object_array_add(jarray,jstring1);
  json_object_array_add(jarray,jstring2);
  json_object_array_add(jarray,jstring3);

  /*Form the json object*/
  /*Each of these is like a key value pair*/
  json_object_object_add(jobj,"Site Name", jstring);
  json_object_object_add(jobj,"Technical blog", jboolean);
  json_object_object_add(jobj,"Average posts per day", jdouble);
  json_object_object_add(jobj,"Number of posts", jint);
  json_object_object_add(jobj,"Categories", jarray);

  /*Now printing the json object*/
  printf ("The json object created: %sn",json_object_to_json_string(jobj));

}