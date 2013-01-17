--TEST--
Properties can be typehinted without defining accessors
--FILE--
<?php

class Test {
    public array $array;
    public stdClass $stdClass;
    public callable $callable;
}

$test = new Test;

$test->array = [];
$test->stdClass = new stdClass;
$test->callable = 'strlen';

var_dump($test->array, $test->stdClass, $test->callable);

set_error_handler(function($errNo, $errStr) { echo $errStr, "\n"; });

$test->array = 41;
$test->stdClass = 42;
$test->callable = 43;

?>
--EXPECT--
array(0) {
}
object(stdClass)#2 (0) {
}
string(6) "strlen"
Argument 1 passed to Test::$array->set() must be of the type array, integer given
Argument 1 passed to Test::$stdClass->set() must be an instance of stdClass, integer given
Argument 1 passed to Test::$callable->set() must be callable, integer given
