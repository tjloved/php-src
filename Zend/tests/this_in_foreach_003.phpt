--TEST--
$this in foreach
--FILE--
<?php

function fn363801805()
{
    $a = [1];
    foreach ($a as &$this) {
        var_dump($this);
    }
}
fn363801805();
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_003.php on line %d
