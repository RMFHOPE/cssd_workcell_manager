#ifndef CSSD_WM_H // include guard
#define CSSD_WM_H


#include <stdlib.h>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <vector>
#include <string>


#include "rmf_msgs/msg/dispenser_request.hpp"
#include "rmf_msgs/msg/dispenser_result.hpp"
#include "rmf_msgs/msg/dispenser_request_item.hpp"
#include "rmf_msgs/msg/inventory_check_request.hpp"
#include "rmf_msgs/msg/inventory_check_response.hpp"
#include "rmf_msgs/msg/dispenser_state.hpp"
#include "rmf_sup_msgs/msg/x_bee_transmit_command.hpp"
#include "rmf_sup_msgs/msg/x_bee_receive_msg.hpp"
#include "std_msgs/msg/int8.hpp"
#include "std_msgs/msg/string.hpp"


using std::placeholders::_1;

struct requests
		{
			std::string request_id;
			std::vector<int32_t> aruco_id;
			std::vector<std::string> item_type;
		};


		class sub_workcell
		{
		public: //dispenser_mode: (0,idle), (1,busy), (2,offline)
			std::string name;
			int32_t dispenser_mode;
			std::string current_trolley_drop_off;
			std::vector<std::string> item_carried_by_RAWM;
			std::vector<requests> queue;
		};



class CssdWorkcellManager : public rclcpp::Node
{
public:
  CssdWorkcellManager(int no_of_workcell);
  void main();
private:

		
  rclcpp::Subscription<rmf_msgs::msg::InventoryCheckRequest>::SharedPtr CheckInventory_;
  rclcpp::Subscription<rmf_msgs::msg::DispenserRequest>::SharedPtr DispenserRequest_;
  rclcpp::Subscription<rmf_msgs::msg::DispenserResult>::SharedPtr RAWMRespond_;  
  rclcpp::Subscription<rmf_msgs::msg::DispenserState>::SharedPtr RAWMState_;    
  rclcpp::Publisher<rmf_msgs::msg::InventoryCheckResponse>::SharedPtr InventoryCheckResponse_;
  rclcpp::Publisher<rmf_msgs::msg::DispenserRequest>::SharedPtr RAWMRequest_;
  rclcpp::Publisher<rmf_msgs::msg::DispenserResult>::SharedPtr DispenserResponsd_;


  sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
  sql::PreparedStatement *pstmt;
	std::string dispenser_name;
	std::string ip_address;
	std::string username;
	std::string password;
	std::string database_name;

	std::vector<sub_workcell> RAWM;
	std::vector<std::string> test;

	bool new_request = false;
	std::string request_id;
	std::string transporter_type;
	

  void inventory_check_callback(const rmf_msgs::msg::InventoryCheckRequest::SharedPtr msg);
  //check inventory when OTUI request for a set of item. Will sort through the requested item and add to the queue of the relevant RAWM

  void RAWM_respond_callback(const rmf_msgs::msg::DispenserResult::SharedPtr msg);
  //update RAWM status when RAWM send response

  void RAWM_state_callback(const rmf_msgs::msg::DispenserState::SharedPtr msg);
  //update RAWM status when RAWM send periodic status updates

  void dispenser_request_callback(const rmf_msgs::msg::DispenserRequest::SharedPtr msg);
  //callback for RFM dispenser request. save the request_id to the cssd_wm class and set "new_request" to true, allowing cssd:main() to start publishing to RAWM

  void failed_loading_handling(std::string request_id);
  //handle RAWM failure. called in cssd_wm::main() when the status of any RAWM is 2.
  //Will release all leftover item in the request_id

  

};


#endif

// for (std::vector<std::string>::iterator it = inventory.begin() ; it != inventory.end(); ++it)
// {
  
//   RCLCPP_INFO(this->get_logger(), *it);
// }