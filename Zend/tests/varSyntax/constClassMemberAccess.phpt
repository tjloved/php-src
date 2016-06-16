--TEST--
Const class member access with deference
--FILE--
<?php

class A
{
    const A = ['a' => ['b' => 'c']];
}
function fn2014467656()
{
    var_dump(A::A);
    var_dump(A::A['a']);
    var_dump(A::A['a']['b']);
}
fn2014467656();
--EXPECT--
array(1) {
  ["a"]=>
  array(1) {
    ["b"]=>
    string(1) "c"
  }
}
array(1) {
  ["b"]=>
  string(1) "c"
}
string(1) "c"