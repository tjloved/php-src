--TEST--
using different variables to access null offsets
--FILE--
<?php

function fn1875138634()
{
    $null = NULL;
    var_dump($null[1]);
    var_dump($null[0.08359999999999999]);
    var_dump($null[NULL]);
    var_dump($null["run away"]);
    var_dump($null[TRUE]);
    var_dump($null[FALSE]);
    $fp = fopen(__FILE__, "r");
    var_dump($null[$fp]);
    $obj = new stdClass();
    var_dump($null[$obj]);
    $arr = array(1, 2, 3);
    var_dump($null[$arr]);
    echo "Done\n";
}
fn1875138634();
--EXPECTF--	
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
Done
