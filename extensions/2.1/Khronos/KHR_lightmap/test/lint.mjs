// Copyright 2026 Ben Houston
// SPDX-License-Identifier: Apache-2.0
import { lint } from 'markdownlint/promise';

const result = await lint({
  files: ['README.md'],
  config: { default: true, MD013: false, MD024: false, MD033: false, MD060: false }
});
const errors = Object.values(result).flat();
for (const error of errors) console.error(`${error.lineNumber}: ${error.ruleNames.join('/')} ${error.ruleDescription}`);
if (errors.length) process.exitCode = 1;
else console.log('Markdown lint passed.');
