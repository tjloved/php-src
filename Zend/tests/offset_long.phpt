--TEST--
using different variables to access long offsets
--FILE--
<?php

function fn297221727()
{
    $long = 1;
    var_dump($long[1]);
    var_dump($long[0.08359999999999999]);
    var_dump($long[NULL]);
    var_dump($long["run away"]);
    var_dump($long[TRUE]);
    var_dump($long[FALSE]);
    $fp = fopen(__FILE__, "r");
    var_dump($long[$fp]);
    $obj = new stdClass();
    var_dump($long[$obj]);
    $arr = array(1, 2, 3);
    var_dump($long[$arr]);
    echo "Done\n";
}
fn297221727();
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
