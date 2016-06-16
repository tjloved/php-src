--TEST--
func_get_args with no args
--FILE--
<?php

function foo()
{
    var_dump(func_get_args());
}
function fn1268194420()
{
    foo();
}
fn1268194420();
--EXPECT--
array(0) {
}
