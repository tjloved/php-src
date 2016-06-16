--TEST--
errmsg: cannot redeclare property
--FILE--
<?php

class test
{
    var $var;
    var $var;
}
function fn1269519996()
{
    echo "Done\n";
}
fn1269519996();
--EXPECTF--	
Fatal error: Cannot redeclare test::$var in %s on line %d
