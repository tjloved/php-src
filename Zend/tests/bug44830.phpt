--TEST--
Bug #44830 (Very minor issue with backslash in heredoc)
--FILE--
<?php

function fn76268695()
{
    $backslash = <<<EOT
\\
EOT;
    var_dump($backslash);
}
fn76268695();
--EXPECT--
string(1) "\"
