--TEST--
$this in unset
--FILE--
<?php

function fn1284585814()
{
    unset($this);
}
fn1284585814();
--EXPECTF--
Fatal error: Cannot unset $this in %sthis_in_unset.php on line %d
