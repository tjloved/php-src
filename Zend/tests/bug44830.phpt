--TEST--
Bug #44830 (Very minor issue with backslash in heredoc)
--FILE--
<?php

function fn1780058757()
{
    $backslash = <<<EOT
\\
EOT;
    var_dump($backslash);
}
fn1780058757();
--EXPECT--
string(1) "\"
