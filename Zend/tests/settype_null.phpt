--TEST--
casting different variables to null using settype() 
--FILE--
<?php

class test
{
    function __toString()
    {
        return "10";
    }
}
function fn1674910967()
{
    $r = fopen(__FILE__, "r");
    $o = new test();
    $vars = array("string", "8754456", "", "\0", 9876545, 0.1, array(), array(1, 2, 3), false, true, NULL, $r, $o);
    foreach ($vars as $var) {
        settype($var, "null");
        var_dump($var);
    }
    echo "Done\n";
}
fn1674910967();
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
NULL
NULL
NULL
NULL
Done
