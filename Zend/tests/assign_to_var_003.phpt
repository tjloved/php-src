--TEST--
complex cases of variable assignment - 003
--FILE--
<?php

function fn1830474889()
{
    $var = 0.213123123;
    $var1 =& $var;
    $var = $var[1];
    var_dump($var);
    var_dump($var1);
    echo "Done\n";
}
fn1830474889();
--EXPECTF--	
NULL
NULL
Done
