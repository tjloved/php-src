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
function fn14967066()
{
    var_dump(method_exists('testclass', 'testfunc'));
    var_dump(method_exists('testclass', 'nonfunc'));
}
fn14967066();
--EXPECT--
bool(true)
bool(false)
