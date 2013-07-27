--TEST--
Return typing test
--FILE--
<?php

class Test {
}

class Demo {
  public function test() : array {
    return array();
  }
  
  public function test2() : stdClass {
      return new stdClass();
  }
  
  public function test3() : Test {
    return new Test();
  }
}

$demo = new Demo();
var_dump($demo->test());
var_dump($demo->test2());
var_dump($demo->test3());
--EXPECT--
array(0) {
}
object(stdClass)#2 (0) {
}
object(Test)#2 (0) {
}