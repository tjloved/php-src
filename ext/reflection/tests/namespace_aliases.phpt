--TEST--
Test getDefinedAliases() on ReflectionFunctionAbstract and ReflectionClass
--FILE--
<?php

use Foo as Bar;
use Foo\Bar as Baz;

function test() { }

class Test {
    public function method() { }
}

$test = function() {};

$fn = new ReflectionFunction('test');
var_dump($fn->getDefinedAliases());

$fn = new ReflectionMethod('Test', 'method');
var_dump($fn->getDefinedAliases());

$fn = new ReflectionFunction($test);
var_dump($fn->getDefinedAliases());

$class = new ReflectionClass('Test');
var_dump($class->getDefinedAliases());

?>
--EXPECT--
array(2) {
  ["bar"]=>
  string(3) "Foo"
  ["baz"]=>
  string(7) "Foo\Bar"
}
array(2) {
  ["bar"]=>
  string(3) "Foo"
  ["baz"]=>
  string(7) "Foo\Bar"
}
array(2) {
  ["bar"]=>
  string(3) "Foo"
  ["baz"]=>
  string(7) "Foo\Bar"
}
array(2) {
  ["bar"]=>
  string(3) "Foo"
  ["baz"]=>
  string(7) "Foo\Bar"
}
