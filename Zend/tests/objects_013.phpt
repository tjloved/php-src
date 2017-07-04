--TEST--
implementing the same interface twice
--FILE--
<?php

interface foo
{
}
class bar implements foo, foo
{
}
function fn285102000()
{
    echo "Done\n";
}
fn285102000();
--EXPECTF--	
Fatal error: Class bar cannot implement previously implemented interface foo in %s on line %d
