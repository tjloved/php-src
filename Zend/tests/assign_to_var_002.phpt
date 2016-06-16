--TEST--
complex cases of variable assignment - 002
--FILE--
<?php

function fn1258019630()
{
    $var = "intergalactic";
    $var1 =& $var;
    $var = $var[5];
    var_dump($var);
    var_dump($var1);
    echo "Done\n";
}
fn1258019630();
--EXPECTF--	
string(1) "g"
string(1) "g"
Done
