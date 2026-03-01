// Wait for the DOM to fully load
document.addEventListener('DOMContentLoaded', () => {
    
    // Copy to Clipboard Functionality
    const copyButtons = document.querySelectorAll('.copy-btn');
    
    copyButtons.forEach(btn => {
        btn.addEventListener('click', () => {
            // Find the associated code block
            const codeBlock = btn.closest('.code-block').querySelector('code');
            const codeText = codeBlock.innerText;
            
            // Navigate clipboard API
            navigator.clipboard.writeText(codeText).then(() => {
                // Change icon to indicate success
                const icon = btn.querySelector('i');
                const originalClass = icon.className;
                
                icon.className = 'ph ph-check';
                icon.style.color = 'var(--accent-primary)';
                
                // Reset after 2 seconds
                setTimeout(() => {
                    icon.className = originalClass;
                    icon.style.color = '';
                }, 2000);
            }).catch(err => {
                console.error('Failed to copy code: ', err);
            });
        });
    });

    // Active State Navigation for Sidebar based on Scroll
    const sections = document.querySelectorAll('section');
    const navLinks = document.querySelectorAll('.sidebar-nav li a');
    
    window.addEventListener('scroll', () => {
        let current = '';
        const scrollY = window.pageYOffset;
        
        sections.forEach(section => {
            const sectionTop = section.offsetTop;
            const sectionHeight = section.clientHeight;
            
            // Allow for a 150px offset to highlight earlier
            if (scrollY >= (sectionTop - 150)) {
                current = section.getAttribute('id');
            }
        });
        
        navLinks.forEach(link => {
            link.classList.remove('active');
            if (link.getAttribute('href').includes(current) && current !== '') {
                link.classList.add('active');
            }
        });
    });

    // Simple keyboard shortcut (/) to focus search box
    const searchInput = document.querySelector('.search-box input');
    
    document.addEventListener('keydown', (e) => {
        if (e.key === '/' && document.activeElement !== searchInput) {
            e.preventDefault(); // Prevent from actually typing '/'
            searchInput.focus();
        }
    });

    // Smooth scroll for anchor links
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function (e) {
            const targetId = this.getAttribute('href');
            
            if(targetId === '#') return;
            
            e.preventDefault();
            const targetElement = document.querySelector(targetId);
            
            if (targetElement) {
                targetElement.scrollIntoView({
                    behavior: 'smooth'
                });
            }
        });
    });
});
