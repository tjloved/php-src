--TEST--
$this in foreach
--FILE--
<?php

function fn1207341775()
{
    $a = [1];
    foreach ($a as $this) {
        var_dump($this);
    }
}
fn1207341775();
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_001.php on line %d
