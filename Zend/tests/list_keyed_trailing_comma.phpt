--TEST--
list() with keys and a trailing comma
--FILE--
<?php

function fn579337748()
{
    $antonyms = ["good" => "bad", "happy" => "sad"];
    list("good" => $good, "happy" => $happy) = $antonyms;
    var_dump($good, $happy);
    echo PHP_EOL;
    $antonyms = ["good" => "bad", "happy" => "sad"];
    list("good" => $good, "happy" => $happy, ) = $antonyms;
    var_dump($good, $happy);
}
fn579337748();
--EXPECT--
string(3) "bad"
string(3) "sad"

string(3) "bad"
string(3) "sad"
