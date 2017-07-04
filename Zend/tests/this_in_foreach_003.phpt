--TEST--
$this in foreach
--FILE--
<?php

function fn2083024129()
{
    $a = [1];
    foreach ($a as &$this) {
        var_dump($this);
    }
}
fn2083024129();
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_003.php on line %d
