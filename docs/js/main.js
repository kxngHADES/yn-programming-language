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

    // Search Engine Logic
    const docSections = document.querySelectorAll('.doc-content section');
    const hrElements = document.querySelectorAll('.doc-content hr');

    searchInput.addEventListener('input', (e) => {
        const query = e.target.value.toLowerCase().trim();
        let hasResults = false;

        docSections.forEach(section => {
            const text = section.innerText.toLowerCase();
            if (text.includes(query)) {
                section.style.display = 'block';
                hasResults = true;
            } else {
                section.style.display = 'none';
            }
        });

        // Hide decorative lines when searching to keep UI clean
        hrElements.forEach(hr => {
            hr.style.display = query ? 'none' : 'block';
        });
        
        // Handle "No results found" state
        let noResultsMsg = document.getElementById('no-results-msg');
        if (!hasResults && query) {
            if (!noResultsMsg) {
                noResultsMsg = document.createElement('div');
                noResultsMsg.id = 'no-results-msg';
                noResultsMsg.style.textAlign = 'center';
                noResultsMsg.style.marginTop = '4rem';
                noResultsMsg.style.color = 'var(--text-muted)';
                noResultsMsg.innerHTML = '<i class="ph ph-magnifying-glass-minus" style="font-size: 3rem; margin-bottom: 1rem; color: var(--border-color); display: inline-block;"></i><br><h3>No results found</h3><p>We couldn\'t find anything matching "<span></span>"</p>';
                document.querySelector('.doc-content').appendChild(noResultsMsg);
            }
            noResultsMsg.style.display = 'block';
            noResultsMsg.querySelector('span').innerText = e.target.value;
        } else if (noResultsMsg) {
            noResultsMsg.style.display = 'none';
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

    // Mobile Menu Toggle Logic
    const mobileMenuBtn = document.getElementById('mobile-menu-btn');
    const sidebar = document.getElementById('sidebar');
    const mobileOverlay = document.getElementById('mobile-overlay');
    const sidebarLinks = document.querySelectorAll('.sidebar-nav a');

    function toggleMobileMenu() {
        sidebar.classList.toggle('active');
        mobileOverlay.classList.toggle('active');
        
        // Change icon based on state
        const icon = mobileMenuBtn.querySelector('i');
        if (sidebar.classList.contains('active')) {
            icon.className = 'ph ph-x';
        } else {
            icon.className = 'ph ph-list';
        }
    }

    if (mobileMenuBtn && sidebar && mobileOverlay) {
        mobileMenuBtn.addEventListener('click', toggleMobileMenu);
        mobileOverlay.addEventListener('click', toggleMobileMenu);

        // Close mobile menu when a link is clicked
        sidebarLinks.forEach(link => {
            link.addEventListener('click', () => {
                if (window.innerWidth <= 768 && sidebar.classList.contains('active')) {
                    toggleMobileMenu();
                }
            });
        });
    }
});
