--TEST--
method_exists() segfaults
--FILE--
<?php

class testclass
{
    function testfunc()
    {
    }
}
function fn1003595116()
{
    var_dump(method_exists('testclass', 'testfunc'));
    var_dump(method_exists('testclass', 'nonfunc'));
}
fn1003595116();
--EXPECT--
bool(true)
bool(false)
