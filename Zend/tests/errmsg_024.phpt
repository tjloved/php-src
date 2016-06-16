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
function fn1812733420()
{
    echo "Done\n";
}
fn1812733420();
--EXPECTF--	
Done
