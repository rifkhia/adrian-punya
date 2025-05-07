<?php
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json");

// Log the raw HTTP request (for debugging)
file_put_contents('request.log', print_r($_SERVER, true) . PHP_EOL . file_get_contents('php://input'));

// Get raw JSON input
$input = file_get_contents('php://input');
file_put_contents('input.log', "Input: " . $input . PHP_EOL, FILE_APPEND);

// Check if input is empty
if (empty($input)) {
    die(json_encode(["error" => "Empty input", "headers" => getallheaders()]));
}

// Decode JSON
$data = json_decode($input, true);
if ($data === null) {
    die(json_encode([
        "error" => "JSON decode failed",
        "raw" => $input,
        "json_last_error" => json_last_error_msg()
    ]));
}

// Validate required fields
if (!isset($data['flow']) || !isset($data['total'])) {
    die(json_encode(["error" => "Missing fields", "data" => $data]));
}

// Save data
file_put_contents('sensor_data.json', json_encode($data));
echo json_encode(["status" => "success"]);
?>