--TEST--
Typehinted properties (without accessors) can have default values
--FILE--
<?php

class Test {
    public array $array = [1, 2, 3];
    public stdClass $object;
    public stdClass $objectNullable = null;
    public callable $callable;
    public callable $callableNullable = null;

    public stdClass $objectAcc {
        get; set { echo __METHOD__."($value)\n"; $this->objectAcc = $value; } 
    }
    public stdClass $objectAccNullable = NULL {
        get; set { echo __METHOD__."($value)\n"; $this->objectAccNullable = $value; } 
    }
}

set_error_handler(function($errNo, $errStr) { echo $errStr, "\n"; });

$test = new Test;
var_dump(
    $test->array, $test->object, $test->objectNullable, $test->callable, $test->callableNullable
);

$test->object = null;
$test->objectNullable = null;
$test->callable = null;
$test->callableNullable = null;

$test->objectAcc = null;
$test->objectAccNullable = null;

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
NULL
NULL
NULL
NULL
Argument 1 passed to Test::$object->set() must be an instance of stdClass, null given
Argument 1 passed to Test::$callable->set() must be callable, null given
Argument 1 passed to Test::$objectAcc->set() must be an instance of stdClass, null given
Test::$objectAcc->set()
Test::$objectAccNullable->set()
