--TEST--
Bug #32428 (The @ warning error suppression operator is broken)
--FILE--
<?php

function fn1782981393()
{
    $data = @$not_exists;
    $data = @$not_exists;
    $data = @(!$not_exists);
    $data = !@$not_exists;
    $data = @($not_exists + 1);
    echo "ok\n";
}
fn1782981393();
--EXPECT--
ok
