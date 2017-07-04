--TEST--
Bug #74810: Something odd about ordering of func_get_args() result in 7.2
--FILE--
<?php

function test_slice1()
{
    var_dump(array_slice(func_get_args(), 1));
}
function test_slice5()
{
    var_dump(array_slice(func_get_args(), 5));
}
function fn815169732()
{
    test_slice1(1, 2, 3);
    test_slice5(1, 2, 3);
}
fn815169732();
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(0) {
}
