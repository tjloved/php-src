--TEST--
errmsg: disabled class
--INI--
disable_classes=stdclass
--FILE--
<?php

class test extends stdclass
{
}
function fn1295746185()
{
    $t = new test();
    echo "Done\n";
}
fn1295746185();
--EXPECTF--	
Warning: test() has been disabled for security reasons in %s on line %d
Done
