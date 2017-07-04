--TEST--
errmsg: can't use [] for reading - 2
--FILE--
<?php

function fn662199946()
{
    $a = array();
    isset($a[]);
    echo "Done\n";
}
fn662199946();
--EXPECTF--	
Fatal error: Cannot use [] for reading in %s on line %d
