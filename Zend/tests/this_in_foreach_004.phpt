--TEST--
$this in foreach
--FILE--
<?php

function fn426025082()
{
    $a = [[1]];
    foreach ($a as list($this)) {
        var_dump($this);
    }
}
fn426025082();
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_004.php on line %d
