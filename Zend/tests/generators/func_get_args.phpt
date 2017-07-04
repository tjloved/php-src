--TEST--
func_get_args() can be used inside generator functions
--FILE--
<?php

function gen()
{
    var_dump(func_get_args());
    yield;
    // trigger generator
}
function fn718839323()
{
    $gen = gen("foo", "bar");
    $gen->rewind();
}
fn718839323();
--EXPECT--
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
