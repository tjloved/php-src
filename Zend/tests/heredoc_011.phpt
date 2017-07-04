--TEST--
STATIC heredocs CAN be used as static scalars.
--FILE--
<?php

class e
{
    const E = <<<THISMUSTNOTERROR
If you DON'T see this, something's wrong.
THISMUSTNOTERROR;
}
function fn1773733675()
{
    require_once 'nowdoc.inc';
    print e::E . "\n";
}
fn1773733675();
--EXPECT--
If you DON'T see this, something's wrong.
