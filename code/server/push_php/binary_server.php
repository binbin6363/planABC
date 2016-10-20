<?php

require_once './Workerman/Autoloader.php';
require_once './Workerman/Protocols/Frame.php';
use Workerman\Worker;

// #### BinaryProtocol worker ####
///$text_worker = new Worker("Frame://0.0.0.0:5678");
$text_worker = new Worker("tcp://0.0.0.0:5678");

$text_worker->onConnect = function($connection)
{
    echo "New connection\n";
};

$text_worker->onMessage =  function($connection, $data)
{
    // send data to client
    $connection->send("hello world $data\n");
};

$text_worker->onClose = function($connection)
{
    echo "Connection closed\n";
};

// run all workers
Worker::runAll();

?>
