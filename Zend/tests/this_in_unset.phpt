--TEST--
$this in unset
--FILE--
<?php

function fn663028071()
{
    unset($this);
}
fn663028071();
--EXPECTF--
Fatal error: Cannot unset $this in %sthis_in_unset.php on line %d
