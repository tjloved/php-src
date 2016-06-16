--TEST--
Testing class_alias() using autoload
--FILE--
<?php

function __autoload($a)
{
    class foo
    {
    }
}
function fn420224046()
{
    class_alias('foo', 'bar', 1);
    var_dump(new foo(), new bar());
}
fn420224046();
--EXPECTF--
object(foo)#%d (0) {
}
object(foo)#%d (0) {
}
