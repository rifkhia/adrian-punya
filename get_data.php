<?php
header("Access-Control-Allow-Origin: *"); 
header("Content-Type: application/json");

// Read saved data
if (file_exists('sensor_data.json')) {
    $data = file_get_contents('sensor_data.json');
    echo $data;
} else {
    echo json_encode(["flow" => 0, "total" => 0]); // Default values
}
?>