'use strict';
// Only the explicit headless-test URL reports an exit status to emrun.
if(location.hash === '#autoclose') {
  (async () => {
    await fetch('stdio.html', {method: 'POST', body: '^pageload^'});
    while(document.body.dataset.complete !== 'true') {
      await new Promise(resolve => setTimeout(resolve, 50));
    }
    const passed = document.querySelectorAll('td[data-state="passed"]').length;
    const failed = document.querySelectorAll('td[data-state="failed"]').length;
    const code = passed === window.feature_tests.length * 2 && failed === 0 && crossOriginIsolated ? 0 : 1;
    for(const cell of document.querySelectorAll('td[data-state="failed"]')) {
      console.error(cell.parentElement.querySelector('.feature').textContent, cell.textContent);
    }
    const detail = `${document.getElementById('summary').textContent}; isolated=${crossOriginIsolated}`;
    await fetch('stdio.html', {method: 'POST', body: `^out^1^${encodeURIComponent(detail)}`});
    await fetch('stdio.html', {method: 'POST', body: `^exit^${code}`});
  })().catch(error => console.error('Smoke runner failed:', error));
}
