--TEST--
$this in foreach
--FILE--
<?php

function fn1141396401()
{
    $a = [[1]];
    foreach ($a as list($this)) {
        var_dump($this);
    }
}
fn1141396401();
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_004.php on line %d
