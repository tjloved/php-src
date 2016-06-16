--TEST--
Bug #39036 (Unsetting key of foreach() yields segmentation fault)
--FILE--
<?php

function fn514204697()
{
    $key = 'asdf';
    foreach (get_defined_vars() as $key => $value) {
        unset(${$key});
    }
    var_dump($key);
    echo "Done\n";
}
fn514204697();
--EXPECTF--	
Notice: Undefined variable: key in %s on line %d
NULL
Done
