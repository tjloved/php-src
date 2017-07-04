--TEST--
using different variables to access boolean offsets
--FILE--
<?php

function fn886811116()
{
    $bool = TRUE;
    var_dump($bool[1]);
    var_dump($bool[0.08359999999999999]);
    var_dump($bool[NULL]);
    var_dump($bool["run away"]);
    var_dump($bool[TRUE]);
    var_dump($bool[FALSE]);
    $fp = fopen(__FILE__, "r");
    var_dump($bool[$fp]);
    $obj = new stdClass();
    var_dump($bool[$obj]);
    $arr = array(1, 2, 3);
    var_dump($bool[$arr]);
    echo "Done\n";
}
fn886811116();
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
