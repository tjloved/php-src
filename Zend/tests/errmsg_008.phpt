--TEST--
errmsg: can't use [] for unsetting
--FILE--
<?php

function fn810212682()
{
    $a = array();
    unset($a[]);
    echo "Done\n";
}
fn810212682();
--EXPECTF--	
Fatal error: Cannot use [] for unsetting in %s on line %d
