--TEST--
complex cases of variable assignment - 003
--FILE--
<?php

function fn805480912()
{
    $var = 0.213123123;
    $var1 =& $var;
    $var = $var[1];
    var_dump($var);
    var_dump($var1);
    echo "Done\n";
}
fn805480912();
--EXPECTF--	
NULL
NULL
Done
