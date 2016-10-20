options = {
    task_center_opion: 
      [
        {
          "address" : {"ip":'127.0.0.1',"port":6300},
          "timeout_interval" : 10000 , // keepalive timeout
          "keepalive_interval" : 5000, // send keepalive interval
          "serverName" : "task_center_server",
          "serverId" : "1111",
          "serverType" : "binary",
          "timeout" : 8000 // backend timeout
        }
      ],
    mysql_opion: 
      [
        {    
          "address" : {"ip":'127.0.0.1',"port":6600},
          "timeout_interval" : 10000 , // keepalive timeout
          "keepalive_interval" : 5000, // send keepalive interval
          "serverName" : "mysql_server",
          "serverId" : "1111",
          "serverType" : "binary",
          "timeout" : 8000 // backend timeout
        } 
      ]
}

exports.options=options
