--TEST--
Bug #39036 (Unsetting key of foreach() yields segmentation fault)
--FILE--
<?php

function fn1297191337()
{
    $key = 'asdf';
    foreach (get_defined_vars() as $key => $value) {
        unset(${$key});
    }
    var_dump($key);
    echo "Done\n";
}
fn1297191337();
--EXPECTF--	
Notice: Undefined variable: key in %s on line %d
NULL
Done
