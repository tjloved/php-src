--TEST--
$this in foreach
--FILE--
<?php

function fn1095495976()
{
    $a = [1];
    foreach ($a as $this) {
        var_dump($this);
    }
}
fn1095495976();
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_001.php on line %d
