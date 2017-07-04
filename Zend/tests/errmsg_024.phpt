--TEST--
No more errmsg: can now change initial value of property
--FILE--
<?php

class test1
{
    protected static $var = 1;
}
class test extends test1
{
    static $var = 10;
}
function fn675841672()
{
    echo "Done\n";
}
fn675841672();
--EXPECTF--	
Done
