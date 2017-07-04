--TEST--
errmsg: can't use [] for unsetting
--FILE--
<?php

function fn590680964()
{
    $a = array();
    unset($a[]);
    echo "Done\n";
}
fn590680964();
--EXPECTF--	
Fatal error: Cannot use [] for unsetting in %s on line %d
