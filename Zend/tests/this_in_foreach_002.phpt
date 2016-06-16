--TEST--
$this in foreach
--FILE--
<?php

function fn705641602()
{
    $a = [1];
    foreach ($a as $this => $dummy) {
        var_dump($this);
    }
}
fn705641602();
--EXPECTF--
Fatal error: Cannot re-assign $this in %sthis_in_foreach_002.php on line %d
